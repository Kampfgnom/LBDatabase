#ifndef LBDATABASE_SINGLETON_H
#define LBDATABASE_SINGLETON_H


#ifdef Q_WS_MAC

#include <QPointer>
#include <QMutex>
#include <QDebug>

#define LB_DECLARE_SINGLETON( Classname ) \
public:\
    static Classname* instance();\
private:\
    static Classname* m_instance;\
    class Classname ## Guard\
    {\
    public:\
        ~Classname ## Guard();\
    };

#define LB_IMPLEMENT_SINGLETON( Classname )\
Classname* Classname::m_instance(0);\
Classname* Classname::instance()\
{\
    static QMutex mutex(QMutex::Recursive);\
    QMutexLocker lock(&mutex);\
    static Classname ## Guard g;\
    if(!m_instance)\
    {\
        m_instance = new Classname();\
    }\
    return m_instance;\
}\
Classname::Classname ## Guard::~Classname ## Guard() \
{\
    qDebug() << "Deleting" << Classname::m_instance;\
    Classname::m_instance->deleteLater();\
}

#endif //Q_WS_MAC

#ifdef Q_OS_LINUX
    #define OS_LINUX true
#else
    #define OS_LINUX false
#endif

#ifdef Q_OS_WIN
    #define OS_WINDOWS true
#else
    #define OS_WINDOWS false
#endif

#if (OS_LINUX || OS_WINDOWS)

#define LB_DECLARE_SINGLETON( NAME )    \
    public: \
    static NAME* instance ()   \
{   \
   static CGuard g;    \
   if (!_instance)   \
      _instance = new NAME ();   \
   return _instance;   \
}   \
private:   \
static NAME* _instance;   \
class CGuard   \
{   \
public:   \
   ~CGuard()   \
   {   \
      if( NULL != NAME::_instance )   \
      {   \
         delete NAME::_instance;   \
         NAME::_instance = NULL;   \
      }   \
   }   \
};   \
friend class CGuard;

#define LB_IMPLEMENT_SINGLETON( NAME )\
    NAME* NAME::_instance = 0;

#endif //Q_OS_WIN

#endif // LBDATABASE_SINGLETON_H
