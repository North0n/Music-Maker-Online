#pragma once

#include <QWidget>
#include <QString>
#include <QValidator>
#include <QHostAddress>
#include <memory>

namespace Ui { class FormCreation; };

class FormCreation : public QWidget
{
	Q_OBJECT

public:
	FormCreation(QWidget *parent = Q_NULLPTR);
	~FormCreation();

signals:
	void onCreatePressed(const QHostAddress& hostname, quint16 port);

private slots:
	void on_pbCreate_clicked();

	void on_pbCancel_clicked();

private:
	Ui::FormCreation* ui;
	std::unique_ptr<QValidator> mIpValidator;
	std::unique_ptr<QValidator> mPortValidator;
};
