#pragma once

#include <QtCore/QString>

class Config
{
    public:
        Config() {}
        ~Config() {}

        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;

        void read();
        void write();

        QString getToolPath(const QString& tool) const;
        const QString& getDotPath() const;
        const QString& getTwopiPath() const;
        const QString& getCircoPath() const;
        const QString& getNeatoPath() const;
        const QString& getFdpPath() const;

    private:
        QString _dotPath{"dot"};
        QString _twopiPath{"twopi"};
        QString _circoPath{"circo"};
        QString _neatoPath{"neato"};
        QString _fdpPath{"fdp"};
};
