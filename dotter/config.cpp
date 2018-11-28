#include <config.h>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QVariant>

void Config::read()
{
    QSettings settings;

    settings.beginGroup("tools");
    _dotPath   = settings.value("dot",   "dot"  ).toString();
    _twopiPath = settings.value("twopi", "twopi").toString();
    _circoPath = settings.value("circo", "circo").toString();
    _neatoPath = settings.value("neato", "neato").toString();
    _fdpPath   = settings.value("fdp",   "fdp"  ).toString();
    settings.endGroup();
}

void Config::write()
{
    QSettings settings;

    if (!settings.isWritable())
    {
        return;
    }

    settings.beginGroup("tools");
    settings.setValue("dot",   _dotPath);
    settings.setValue("twopi", _twopiPath);
    settings.setValue("circo", _circoPath);
    settings.setValue("neato", _neatoPath);
    settings.setValue("fdp",   _fdpPath);
    settings.endGroup();
}

QString Config::getToolPath(const QString& tool) const
{
    if (tool == "dot")
    {
        return getDotPath();
    }
    else if (tool == "twopi")
    {
        return getTwopiPath();
    }
    else if (tool == "circo")
    {
        return getCircoPath();
    }
    else if (tool == "neato")
    {
        return getNeatoPath();
    }
    else if (tool == "fdp")
    {
        return getFdpPath();
    }

    return QString{};
}

const QString& Config::getDotPath() const
{
    return _dotPath;
}

const QString& Config::getTwopiPath() const
{
    return _twopiPath;
}

const QString& Config::getCircoPath() const
{
    return _circoPath;
}

const QString& Config::getNeatoPath() const
{
    return _neatoPath;
}

const QString& Config::getFdpPath() const
{
    return _fdpPath;
}
