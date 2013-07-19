/*
 * Reader.h
 *
 *  Created on: 19/lug/2013
 *      Author: stefano
 */

#ifndef READER_H_
#define READER_H_


#include <list>

template<typename T>
class ItemsList {
public:
	ItemsList() { Rewind(); };
	virtual ~ItemsList() {};

	void Rewind() { fIterator = fItems.begin(); };
	bool GetNext(T& item) {
		if (fIterator == fItems.end())
			return false;

		item = *fIterator;
		fIterator++;
		return true;
	}
protected:
	std::list<T> fItems;
	typename std::list<T>::iterator fIterator;
};

#endif /* READER_H_ */
