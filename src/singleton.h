
#ifndef UAV_Singleton_h
#define UAV_Singleton_h

/*
Singletones can be called from any point of application code.
*/

template <class T> class singleton_base
{
	/*
	Base for singleton with static variable instance.
	*/
public:
	static T * setSingleton(T* instance)
	{
		m_pInstance = instance;
		return m_pInstance;
	}

protected:
	singleton_base();
	virtual ~singleton_base();
protected:
	static T * m_pInstance;
};

template <class T> T* singleton_base<T>::m_pInstance = 0;


template <class T> class singleton : public singleton_base<T>
{
	/*
	Singleton with manual creation. First time you must
	create the instance and call setSingleton().
	*/
public:
	static T* get_singleton()
	{
		return singleton_base<T>::m_pInstance;
	}
};

template <class T> class autocreate_singleton : public singleton_base<T>
{
public:
	/*
	Singleton with automatic creation on first call.
	*/
	static T* get_singleton()
	{
		if (!singleton_base<T>::m_pInstance)
			singleton_base<T>::m_pInstance = new T;
		return singleton_base<T>::m_pInstance;
	}
};

#endif
