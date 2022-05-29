#include "FormConnection.h"
#include "ui_FormConnection.h"

#include <QIntValidator>
#include <QRegularExpression>

FormConnection::FormConnection(QWidget *parent)
	: QWidget(parent),
    ui(new Ui::FormConnection)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowModality(Qt::WindowModal);
    //setModal(true);

    ui->setupUi(this);

    mPortValidator = std::make_unique<QIntValidator>(0, UINT16_MAX, this);
    ui->lePort->setValidator(mPortValidator.get());

    // Regular expression for valid IPv4 address
    QRegularExpression regExp(R"(^((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$)");
    mIpValidator = std::make_unique<QRegularExpressionValidator>(regExp, this);
    ui->leIp->setValidator(mIpValidator.get());
}

FormConnection::~FormConnection()
{
	delete ui;
}

void FormConnection::on_pbCancel_clicked()
{
    if (ui->leIp->hasAcceptableInput() && ui->lePort->hasAcceptableInput())
        emit onConnectPressed(QHostAddress(ui->leIp->text()), ui->lePort->text().toInt());
    close();
}

void FormConnection::on_pbConnect_clicked()
{
    close();
}