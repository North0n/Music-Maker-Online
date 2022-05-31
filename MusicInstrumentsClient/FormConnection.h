#pragma once

#include <QWidget>
#include <QString>
#include <QValidator>
#include <QHostAddress>
#include <memory>
#include <QKeyEvent>

namespace Ui { class FormConnection; };

class FormConnection : public QWidget
{
	Q_OBJECT

public:
	FormConnection(QWidget *parent = Q_NULLPTR);
	~FormConnection();

signals:
	void onConnectPressed(const QHostAddress& hostname, quint16 port);


protected:

	void keyPressEvent(QKeyEvent* event) override;

private slots:
	void on_pbConnect_clicked();

	void on_pbCancel_clicked();

private:
	Ui::FormConnection *ui;
	std::unique_ptr<QValidator> mIpValidator;
	std::unique_ptr<QValidator> mPortValidator;
};
