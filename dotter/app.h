#pragma once

#include <QApplication>
class Config;

class App : public QApplication {
Q_OBJECT

public:
	App(int& argc, char** argv);
	virtual ~App();

	App() = delete;
	App(const App&) = delete;
	App& operator=(const App&) = delete;

	Config* config();

public slots:
	void quitting();

private:
	Config* _config{nullptr};
};
