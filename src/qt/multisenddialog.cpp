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
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

MultiSendDialog::MultiSendDialog(QWidget* parent) : QDialog(parent),
                                                    ui(new Ui::MultiSendDialog),
                                                    model(0)
{
    ui->setupUi(this);

    updateCheckBoxes();
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
	/*if (pwalletMain->fMultiSendStake && pwalletMain->fMultiSendMasternodeReward) {
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
	*/
}

void MultiSendDialog::on_addressBookButton_clicked()
{
	if (model && model->getAddressTableModel()) {
		AddressBookPage dlg(AddressBookPage::ForSelection, AddressBookPage::ReceivingTab, this);
		dlg.setModel(model->getAddressTableModel());
		if (dlg.exec()) {
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
			frameLayout->setSpacing(1);
			frameLayout->setObjectName(QStringLiteral("frameLayout"));
			frameLayout->setContentsMargins(6, 6, 6, 6);

			QHBoxLayout* addressLayout = new QHBoxLayout();
			addressLayout->setSpacing(0);
			addressLayout->setObjectName(QStringLiteral("addressLayout"));

			QLabel* addressLabel = new QLabel(addressFrame);
			addressLabel->setObjectName(QStringLiteral("addressLabel"));
			addressLabel->setText(dlg.getReturnValue());
			addressLayout->addWidget(addressLabel);
			
			QPushButton* addressConfigureButton = new QPushButton(addressFrame);
			addressConfigureButton->setObjectName(QStringLiteral("addressConfigureButton"));
			QIcon icon4;
			icon4.addFile(QStringLiteral(":/icons/edit"), QSize(), QIcon::Normal, QIcon::Off);
			addressDeleteButton->setIcon(icon4);
			addressDeleteButton->setAutoDefault(false);
			connect(addressConfigureButton, SIGNAL(clicked()), this, SLOT(blank())); //TODO write method for configuring
			addressLayout->addWidget(addressConfigureButton);

			QPushButton* addressDeleteButton = new QPushButton(addressFrame);
			addressDeleteButton->setObjectName(QStringLiteral("addressDeleteButton"));
			QIcon icon5;
			icon5.addFile(QStringLiteral(":/icons/remove"), QSize(), QIcon::Normal, QIcon::Off);
			addressDeleteButton->setIcon(icon5);
			addressDeleteButton->setAutoDefault(false);
			connect(addressDeleteButton, SIGNAL(clicked()), this, SLOT(deleteFrame()));

			addressLayout->addWidget(addressDeleteButton);
			frameLayout->addLayout(addressLayout);
			ui->addressList->addWidget(addressFrame);
		}
	}

}

void MultiSendDialog::blank() {
	return;
}

void MultiSendDialog::deleteFrame() {
	
		QWidget *buttonWidget = qobject_cast<QWidget*>(sender());
		if (!buttonWidget)return;

		QFrame* frame = qobject_cast<QFrame*>(buttonWidget->parentWidget());
		if (!frame)return;

		delete frame;
	
}

void MultiSendDialog::on_activateButton_clicked() //TODO actually have an output for failure
{
    std::string strRet = "";
    if (!(ui->multiSendStakeCheckBox->isChecked() || ui->multiSendMasternodeCheckBox->isChecked())) {
        strRet = "Need to select to send on stake and/or masternode rewards\n";
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
    return;
}
