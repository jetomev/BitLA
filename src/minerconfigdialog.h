#pragma once

#include <QDialog>
#include <QString>

class QComboBox;
class QLineEdit;
class QSpinBox;

class MinerConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit MinerConfigDialog(QWidget *parent = nullptr);

    QString poolUrl() const;
    QString walletAddress() const;
    QString algo() const;
    QString threads() const;

private:
    QLineEdit *poolUrlEdit = nullptr;
    QLineEdit *walletEdit = nullptr;
    QLineEdit *userEdit = nullptr;
    QLineEdit *passwordEdit = nullptr;
    QComboBox *algoCombo = nullptr;
    QSpinBox *threadsSpin = nullptr;
};

#endif // MINERCONFIGDIALOG_H
