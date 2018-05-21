#include "multisendconfigdialog.h"
#include "ui_multisendconfigdialog.h"
#include "addressbookpage.h"
#include "addresstablemodel.h"
#include "base58.h"
#include "init.h"
#include "walletmodel.h"
#include "qvalidatedlineedit.h"
#include <QFrame>
#include <QString>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QSpinBox>
#include <QClipboard>
#include <algorithm>



using namespace std;
using namespace boost;

MultiSendConfigDialog::MultiSendConfigDialog(QWidget* parent, std::string addy, std::vector<std::pair<std::string, int>>* addressEntry) : QDialog(parent),
																address(addy),
																vMultiSendAddressEntry(addressEntry),
																ui(new Ui::MultiSendConfigDialog),
																model(0)
{
    ui->setupUi(this);

    updateStatus();
}

MultiSendConfigDialog::~MultiSendConfigDialog()
{
    delete ui;
}

void MultiSendConfigDialog::setModel(WalletModel* model)
{
    this->model = model;
}

void MultiSendConfigDialog::updateStatus()
{   

}

void MultiSendConfigDialog::on_addEntryButton_clicked()
{
	if (model) {
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

		QValidatedLineEdit* addressLine = new QValidatedLineEdit(addressFrame);
		addressLine->setObjectName(QStringLiteral("addressLabel"));
		addressLayout->addWidget(addressLine);

		QSpinBox* percentageSpinBox = new QSpinBox(addressFrame);
		percentageSpinBox - setObjectName(QStringLiteral("percentageSpinBox"));
		addressLayout->addWidget(percentageSpinBox);

		QPushButton* sendingAddressButton = new QPushButton(addressFrame);
		sendingAddressButton->setObjectName(QStringLiteral("sendingAddressButton"));
		QIcon icon1;
		icon1.addFile(QStringLiteral(":/icons/address-book"), QSize(), QIcon::Normal, QIcon::Off);
		sendingAddressButton->setIcon(icon1);
		sendingAddressButton->setAutoDefault(false);
		connect(sendingAddressButton, SIGNAL(clicked()), this, SLOT(selectSendingAddress()));
		addressLayout->addWidget(sendingAddressButton);

		QPushButton* pasteButton = new QPushButton(addressFrame);
		pasteButton->setObjectName(QStringLiteral("pasteButton"));
		QIcon icon2;
		icon2.addFile(QStringLiteral(":/icons/editpaste"), QSize(), QIcon::Normal, QIcon::Off);
		pasteButton->setIcon(icon2);
		pasteButton->setAutoDefault(false);
		connect(addressDeleteButton, SIGNAL(clicked()), this, SLOT(pasteText()));
		addressLayout->addWidget(pasteButton);			

		QPushButton* addressDeleteButton = new QPushButton(addressFrame);
		addressDeleteButton->setObjectName(QStringLiteral("addressDeleteButton"));
		QIcon icon3;
		icon3.addFile(QStringLiteral(":/icons/remove"), QSize(), QIcon::Normal, QIcon::Off);
		addressDeleteButton->setIcon(icon3);
		addressDeleteButton->setAutoDefault(false);
		connect(addressDeleteButton, SIGNAL(clicked()), this, SLOT(deleteFrame()));
		addressLayout->addWidget(addressDeleteButton);

		frameLayout->addLayout(addressLayout);
		ui->addressList->addWidget(addressFrame);
}
}

void MultiSendConfigDialog::selectSendingAddress()
{	
		QWidget* addressButton = qobject_cast<QWidget*>(sender());
		if (!addressButton)return;

		QFrame* addressFrame = qobject_cast<QFrame*>(addressButton->parentWidget());
		if (!addressFrame)return;

		QValidatedLineEdit* vle = addressFrame->findChild<QValidatedLineEdit*>("addressLine");
		if (!vle)return;

		if (model && model->getAddressTableModel()) {
			AddressBookPage dlg(AddressBookPage::ForSelection, AddressBookPage::SendingTab, this);
			dlg.setModel(model->getAddressTableModel());
			if (dlg.exec()) {
				vle->setText(dlg.getReturnValue());
			}
		}	
}

void MultiSendConfigDialog::pasteText()
{
	QWidget* pasteButton = qobject_cast<QWidget*>(sender());
	if (!pasteButton)return;

	QFrame* addressFrame = qobject_cast<QFrame*>(pasteButton->parentWidget());
	if (!addressFrame)return;

	QValidatedLineEdit* vle = addressFrame->findChild<QValidatedLineEdit*>("addressLine");
	if (!vle)return;

	vle->setText(QApplication::clipboard()->text());
}


void MultiSendConfigDialog::deleteFrame() {
	
		QWidget *buttonWidget = qobject_cast<QWidget*>(sender());
		if (!buttonWidget)return;

		QFrame* frame = qobject_cast<QFrame*>(buttonWidget->parentWidget());
		if (!frame)return;

		delete frame;
	
}

void MultiSendConfigDialog::on_activateButton_clicked() //TODO actually have an output for failure
{
   
}

void MultiSendConfigDialog::on_disableButton_clicked()
{
  
}
