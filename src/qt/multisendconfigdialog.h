#ifndef MULTISENDCONFIGDIALOG_H
#define MULTISENDCONFIGDIALOG_H

#include <QDialog>
#include <utility>
#include <vector>

namespace Ui
{
class MultiSendConfigDialog;
}

class WalletModel;
class MultiSendConfigDialog : public QDialog
{
    Q_OBJECT
    void updateStatus();

private:
	Ui::MultiSendConfigDialog* ui;
	WalletModel* model;
	std::string address;
	std::vector<std::pair<std::string, int>>* vMultiSendAddressEntry;

public:
    explicit MultiSendConfigDialog(QWidget* parent = 0, std::string addy = "NaN", std::vector<std::pair<std::string, int>>* addressEntry);
    ~MultiSendConfigDialog();
    void setModel(WalletModel* model);
    
private slots:
	void deleteFrame();
    void on_activateButton_clicked();
    void on_disableButton_clicked();
    void on_addressBookButton_clicked();


};

#endif // MULTISENDDIALOG_H
