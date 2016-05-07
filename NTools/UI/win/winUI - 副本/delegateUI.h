#pragma once
namespace wui{

class WINUI_API delegateBase	 
{
public:
	delegateBase(void* pObject, void* pFn);
	delegateBase(const delegateBase& rhs);
	virtual ~delegateBase();
	bool equals(const delegateBase& rhs) const;
	bool operator() (void* param);
	virtual delegateBase* copy() = 0;

protected:
	void* getFn();
	void* getObject();
	virtual bool invoke(void* param) = 0;

private:
	void* m_pObject;
	void* m_pFn;
};

class delegateStatic: public delegateBase
{
	typedef bool (*Fn)(void*);
public:
	delegateStatic(Fn pFn) : delegateBase(NULL, pFn) { } 
	delegateStatic(const delegateStatic& rhs) : delegateBase(rhs) { } 
	virtual delegateBase* copy() { return new delegateStatic(*this); }

protected:
	virtual bool invoke(void* param)
	{
		Fn pFn = (Fn)getFn();
		return (*pFn)(param); 
	}
};

template <class O, class T>
class delegate : public delegateBase
{
	typedef bool (T::* Fn)(void*);
public:
	delegate(O* pObj, Fn pFn) : delegateBase(pObj, &pFn), m_pFn(pFn) { }
	delegate(const delegate& rhs) : delegateBase(rhs) { m_pFn = rhs.m_pFn; } 
	virtual delegateBase* copy() { return new delegate(*this); }

protected:
	virtual bool invoke(void* param)
	{
		O* pObject = (O*) getObject();
		return (pObject->*m_pFn)(param); 
	}  

private:
	Fn m_pFn;
};

template <class O, class T>
delegate<O, T> makeDelegate(O* pObject, bool (T::* pFn)(void*))
{
	return delegate<O, T>(pObject, pFn);
}

inline delegateStatic makeDelegate(bool (*pFn)(void*))
{
	return delegateStatic(pFn); 
}

class WINUI_API eventSource
{
	typedef bool (*FnType)(void*);
public:
	~eventSource();
	operator bool();
	void operator+= (delegateBase& d);
	void operator+= (FnType pFn);
	void operator-= (delegateBase& d);
	void operator-= (FnType pFn);
	bool operator() (void* param);

protected:
	std::vector<delegateBase *> m_aDelegates;
};

}