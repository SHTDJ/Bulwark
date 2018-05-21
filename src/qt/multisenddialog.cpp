#include "multisenddialog.h"
#include "ui_multisenddialog.h"
#include "addressbookpage.h"
#include "addresstablemodel.h"
#include "base58.h"
#include "init.h"
#include "walletmodel.h"
#include <QFrame>
#include <QString>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QMessageBox>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <utility>

using namespace std;
using namespace boost;

MultiSendDialog::MultiSendDialog(QWidget* parent) : QDialog(parent),
                                                    ui(new Ui::MultiSendDialog),
                                                    model(0)
{
    ui->setupUi(this);
    updateCheckBoxes();
	for (int i = 0; i < pwalletMain->vMultiSend.size(); i++) {
		addAddress(pwalletMain->vMultiSend[i].first,true);
	}
}

MultiSendDialog::~MultiSendDialog()
{
    delete ui;
}

void MultiSendDialog::setModel(WalletModel* model)
{
    this->model = model;
}

void MultiSendDialog::updateCheckBoxes()
{
	ui->multiSendStakeCheckBox->setChecked(pwalletMain->fMultiSendStake);
	ui->multiSendMasternodeCheckBox->setChecked(pwalletMain->fMultiSendMasternodeReward);
	updateStatus();
}

void MultiSendDialog::updateStatus()
{   
	if (pwalletMain->fMultiSendStake && pwalletMain->fMultiSendMasternodeReward) {
		ui->multiSendStatusLabel->setText(QStringLiteral("Enabled for Staking and Masternodes"));
	}
	else if (pwalletMain->fMultiSendStake) {
		ui->multiSendStatusLabel->setText(QStringLiteral("Enabled for Staking"));
	}
	else if (pwalletMain->fMultiSendMasternodeReward) {
		ui->multiSendStatusLabel->setText(QStringLiteral("Enabled for Masternodes"));
	}
	else {
		ui->multiSendStatusLabel->setText(QStringLiteral("Disabled"));
	}
	
}

void MultiSendDialog::on_addressBookButton_clicked()
{
	if (model && model->getAddressTableModel()) {
		AddressBookPage dlg(AddressBookPage::ForSelection, AddressBookPage::ReceivingTab, this);
		dlg.setModel(model->getAddressTableModel());
		if (dlg.exec()) {
			addAddress(dlg.getReturnValue().toStdString(), false);
		}
	}

}

void MultiSendDialog::addAddress(std::string address, bool onLoad) {
	if (!onLoad){
		std::pair<std::string, std::vector<std::pair<std::string, int>>> pMultiSendAddress;
		std::vector<std::pair<std::string, int>> vAddressConfig;
		pwalletMain->vMultiSend.push_back(std::make_pair(address, vAddressConfig));
		CWalletDB walletdb(pwalletMain->strWalletFile);
		walletdb.WriteMultiSend(pwalletMain->vMultiSend);
	}

	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	QFrame* addressFrame = new QFrame();
	sizePolicy.setHeightForWidth(addressFrame->sizePolicy().hasHeightForWidth());
	addressFrame->setSizePolicy(sizePolicy);
	addressFrame->setFrameShape(QFrame::StyledPanel);
	addressFrame->setFrameShadow(QFrame::Raised);
	addressFrame->setObjectName(QStringLiteral("addressFrame"));

	QVBoxLayout* frameLayout = new QVBoxLayout(addressFrame);
	frameLayout->setSpacing(2);
	frameLayout->setObjectName(QStringLiteral("frameLayout"));
	frameLayout->setContentsMargins(6, 6, 6, 6);

	QHBoxLayout* addressLayout = new QHBoxLayout();
	addressLayout->setSpacing(4);
	addressLayout->setObjectName(QStringLiteral("addressLayout"));

	QLabel* addressLabel = new QLabel(addressFrame);
	addressLabel->setObjectName(QStringLiteral("addressLabel"));
	addressLabel->setText(QString::fromStdString(address));
	addressLayout->addWidget(addressLabel);

	QPushButton* addressConfigureButton = new QPushButton(addressFrame);
	addressConfigureButton->setObjectName(QStringLiteral("addressConfigureButton"));
	QIcon icon1;
	icon1.addFile(QStringLiteral(":/icons/edit"), QSize(), QIcon::Normal, QIcon::Off);
	addressConfigureButton->setIcon(icon1);
	addressConfigureButton->setAutoDefault(false);
	connect(addressConfigureButton, SIGNAL(clicked()), this, SLOT(configureMultiSend())); //TODO write method for configuring
	addressLayout->addWidget(addressConfigureButton);

	QPushButton* addressDeleteButton = new QPushButton(addressFrame);
	addressDeleteButton->setObjectName(QStringLiteral("addressDeleteButton"));
	QIcon icon2;
	icon2.addFile(QStringLiteral(":/icons/remove"), QSize(), QIcon::Normal, QIcon::Off);
	addressDeleteButton->setIcon(icon2);
	addressDeleteButton->setAutoDefault(false);
	connect(addressDeleteButton, SIGNAL(clicked()), this, SLOT(deleteFrame()));
	addressLayout->addWidget(addressDeleteButton);

	
	frameLayout->addLayout(addressLayout);
	ui->addressList->addWidget(addressFrame);
}

void MultiSendDialog::configureMultiSend() {
	QWidget *buttonWidget = qobject_cast<QWidget*>(sender());
	if (!buttonWidget)return;

	QFrame* frame = qobject_cast<QFrame*>(buttonWidget->parentWidget());
	if (!frame)return;
	QLabel* lbl = addressFrame->findChild<QLabel*>("addressLabel");

	if (!lbl)return;
	std::string address = lbl->text().toStdString();
	std::vector<std::pair<std::string, int>>* multiSendAddressEntry = vMultiSend[pwalletMain->indexOfMSAddress(address)];
	MultiSendConfigDialog* multiSendConfigDialog = new MultiSendConfigDialog(this,address, multiSendAddressEntry);
}

void MultiSendDialog::deleteFrame() {
	
		QWidget *buttonWidget = qobject_cast<QWidget*>(sender());
		if (!buttonWidget)return;

		QFrame* frame = qobject_cast<QFrame*>(buttonWidget->parentWidget());
		if (!frame)return;
		CWalletDB walletdb(pwalletMain->strWalletFile);
		walletdb.EraseMultiSend(pwalletMain->vMultiSend);
		QLabel* lbl = addressFrame->findChild<QLabel*>("addressLabel");
		pwalletMain->deleteMSAddress(lbl->text().toStdString());
		CWalletDB walletdb(pwalletMain->strWalletFile);
		walletdb.EraseMultiSend(pwalletMain->vMultiSend);
		walletdb.WriteMultiSend(pwalletMain->vMultiSend);

		delete frame;
	
}

void MultiSendDialog::on_activateButton_clicked()
{
    std::string strRet = "";
    if (!(ui->multiSendStakeCheckBox->isChecked() || ui->multiSendMasternodeCheckBox->isChecked())) {
		QMessageBox::information(this, tr("MultiSend Status"),
			tr("Need to select to send for either staking or masternode rewards"),
			QMessageBox::Ok, QMessageBox::Ok);
    } else {
        pwalletMain->fMultiSendStake = ui->multiSendStakeCheckBox->isChecked();
        pwalletMain->fMultiSendMasternodeReward = ui->multiSendMasternodeCheckBox->isChecked();

        CWalletDB walletdb(pwalletMain->strWalletFile);
        if (!walletdb.WriteMSettings(pwalletMain->fMultiSendStake, pwalletMain->fMultiSendMasternodeReward, pwalletMain->nLastMultiSendHeight))
            strRet = "MultiSend activated but writing settings to DB failed";
        else
            strRet = "MultiSend activated";
    }
	updateStatus();
    return;
}

void MultiSendDialog::on_disableButton_clicked()
{
    std::string strRet;
    pwalletMain->setMultiSendDisabled();
    CWalletDB walletdb(pwalletMain->strWalletFile);
    if (!walletdb.WriteMSettings(false, false, pwalletMain->nLastMultiSendHeight))
        strRet = "MultiSend deactivated but writing settings to DB failed";
    else
        strRet = "MultiSend deactivated";
	updateStatus();
    return;
}
