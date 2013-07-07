#ifndef X11_H
#define X11_H
#include <QString>
#include <QList>
#include <QWidget>


namespace skypetab
{
namespace X11
{

	extern Display* XDisplay;
	QString GetWindowName(WId window);
	QList<WId> GetChildren(WId window);
	WId GetInputFocus();
	void Flush();
	void Sync(bool discard);
	void SetClassName(WId window, QString className);
	WId FindWindowByClass(QString className);
	void SendXMessage(WId win, QString message);
	void SetupErrorHandler();
}

}
#endif // X11_H
