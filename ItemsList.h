/*
 * ItemsList.h
 *
 *  Created on: 19/lug/2013
 *      Author: Stefano Ceccherini (stefano.ceccherini@gmail.com)
 */

#ifndef ITEMSLIST_H_
#define ITEMSLIST_H_

#include <list>

template<typename T>
class ItemsList {
public:
	ItemsList() { fIterator = fItems.end(); };
	virtual ~ItemsList() {};
	
	void Rewind() { fIterator = fItems.begin(); };
	bool GetNext(T& item) {
		if (fIterator == fItems.end())
			return false;

		item = *fIterator;
		fIterator++;
		return true;
	};
	
	// Those are O(n) but added because they're handy
	T& operator[](int i) const {
		typename std::list<T>::const_iterator it = fItems.begin();
		std::advance(it, i);
		return *it; 
	}
	std::size_t Count() const { return fItems.size(); };
	
protected:
	std::list<T> fItems;
	typename std::list<T>::iterator fIterator;
};

#endif /* ITEMSLIST_H_ */
