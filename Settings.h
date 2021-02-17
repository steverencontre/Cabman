#ifndef VALUES_H
#define VALUES_H

#include <QSettings>
#include <QRect>
#include <QUuid>
#include <QString>


template <typename T> T toType (const QVariant& v);

#define TO_TYPE(ctype, qname)	\
template<> \
inline ctype toType<ctype> (const QVariant& v) { return v.to##qname(); }

#define TO_QTYPE(type)		TO_TYPE (Q##type, type)

TO_TYPE		(int, Int);
TO_QTYPE		(Rect);
TO_QTYPE		(ByteArray);
TO_QTYPE		(String);

#define VALUE(T,N)	\
	T get##N (const QVariant& def={}) const		{ return toType<T> (value (#N, def)); }		\
	void set##N (T t)											{ setValue (#N, t); }


class Settings : private QSettings
{
public:
	Settings() = default;
	~Settings() override;

	VALUE (QString,			DatabaseRoot)
	VALUE (QRect,			WindowPos)
	VALUE (int,				LeftRightSplit)
	VALUE (int,				InOutSplit)
	VALUE (int,				ThreadMsgSplit)
	VALUE (QByteArray,	CixSyncDeltaId)

};

#endif // VALUES_H

