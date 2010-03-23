#include <map>
#include <iterator>

template <typename BaseClass>
class genericFactory {
private:
	typedef BaseClass * (*factoryCreator)(QWidget *parent , const char *widgetName);

	static std::map<QString,factoryCreator> &registered() {
				static std::map<QString,factoryCreator> handler;
				return handler;
		};
	QString myname;
public:
	// register and deregister a constructor for ancestor BaseClass
	genericFactory(const QString &name, factoryCreator fn)
			{
				registered()[name] = fn;
				myname = name;
			};
	~genericFactory() {
				registered().erase(myname);
			};

	// create a derived class of BaseClass
	static BaseClass *Create(const QString &name, QWidget *parent=NULL , const char *widgetName=NULL )
			{
				typename std::map<QString,factoryCreator>::iterator it = registered().find(name);
				if( it != registered().end() )
					return it->second(parent, widgetName);
				return NULL;
			};
	static std::vector<QString> getFactoryList() {
				typename std::map<QString,factoryCreator>::iterator it = registered().begin();
				std::vector<QString> rval;
				while( it != registered().end() )
					rval.push_back(it->first);
				return rval;
			};
	static QString getFactoryNames(QString delim = ",") {
				typename std::map<QString,factoryCreator>::iterator it = registered().begin();
				QString rval;
				while( it != registered().end() )
					rval += it->first + delim;
				return rval; 
			};
};
