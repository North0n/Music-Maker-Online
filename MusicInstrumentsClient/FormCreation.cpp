#include "FormCreation.h"
#include "ui_FormCreation.h"

#include <QIntValidator>
#include <QRegularExpression>
#include <QKeyEvent>

FormCreation::FormCreation(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::FormCreation)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowModality(Qt::WindowModal);

    ui->setupUi(this);

    mPortValidator = std::make_unique<QIntValidator>(0, UINT16_MAX, this);
    ui->lePort->setValidator(mPortValidator.get());

    // Regular expression for valid IPv4 address
    QRegularExpression regExp(R"(^((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$)");
    mIpValidator = std::make_unique<QRegularExpressionValidator>(regExp, this);
    ui->leIp->setValidator(mIpValidator.get());
}

FormCreation::~FormCreation()
{
	delete ui;
}

void FormCreation::on_pbCreate_clicked()
{
    if (ui->leIp->hasAcceptableInput() && ui->lePort->hasAcceptableInput())
        emit onCreatePressed(QHostAddress(ui->leIp->text()), ui->lePort->text().toInt());
    close();
}

void FormCreation::on_pbCancel_clicked()
{
    close();
}

void FormCreation::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        on_pbCancel_clicked();
    }
}