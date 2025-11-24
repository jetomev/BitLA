#include "minerconfigdialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>

MinerConfigDialog::MinerConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Configure Miner"));

    auto *layout = new QVBoxLayout(this);
    auto *form = new QFormLayout;

    poolUrlEdit = new QLineEdit;
    poolUrlEdit->setPlaceholderText(tr("stratum+tcp://pool:port"));
    form->addRow(tr("Pool URL"), poolUrlEdit);

    walletEdit = new QLineEdit;
    walletEdit->setPlaceholderText(tr("Wallet address"));
    form->addRow(tr("Wallet"), walletEdit);

    userEdit = new QLineEdit;
    form->addRow(tr("User"), userEdit);

    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);
    form->addRow(tr("Password"), passwordEdit);

    algoCombo = new QComboBox;
    algoCombo->addItems({"scrypt", "sha256d"});
    form->addRow(tr("Algorithm"), algoCombo);

    threadsSpin = new QSpinBox;
    threadsSpin->setMinimum(1);
    threadsSpin->setMaximum(256);
    threadsSpin->setValue(4);
    form->addRow(tr("Threads"), threadsSpin);

    layout->addLayout(form);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &MinerConfigDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &MinerConfigDialog::reject);
    layout->addWidget(buttons);
}

QString MinerConfigDialog::poolUrl() const
{
    return poolUrlEdit ? poolUrlEdit->text() : QString();
}

QString MinerConfigDialog::walletAddress() const
{
    return walletEdit ? walletEdit->text() : QString();
}

QString MinerConfigDialog::algo() const
{
    return algoCombo ? algoCombo->currentText() : QString();
}

QString MinerConfigDialog::threads() const
{
    return threadsSpin ? QString::number(threadsSpin->value()) : QString();
}
