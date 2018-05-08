#ifndef MULTISENDDIALOG_H
#define MULTISENDDIALOG_H

#include <QDialog>

namespace Ui
{
class MultiSendDialog;
}

class WalletModel;
class MultiSendDialog : public QDialog
{
    Q_OBJECT
    void updateStatus();
	void updateCheckBoxes();

public:
    explicit MultiSendDialog(QWidget* parent = 0);
    ~MultiSendDialog();
    void setModel(WalletModel* model);
    
private slots:
	void deleteFrame();
    void on_activateButton_clicked();
    void on_disableButton_clicked();
    void on_addressBookButton_clicked();

private:
    Ui::MultiSendDialog* ui;
    WalletModel* model;
};

#endif // MULTISENDDIALOG_H
