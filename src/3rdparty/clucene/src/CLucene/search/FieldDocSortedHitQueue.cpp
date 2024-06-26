/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "FieldDocSortedHitQueue.h"


CL_NS_USE(util)
CL_NS_DEF(search)


FieldDoc::FieldDoc (int32_t doc, qreal score)
{
	this->scoreDoc.doc = doc;
	this->scoreDoc.score = score;
	fields=nullptr;
}

FieldDoc::FieldDoc (int32_t doc, qreal score, CL_NS(util)::Comparable** fields)
{
	this->scoreDoc.doc = doc;
	this->scoreDoc.score = score;
	this->fields = fields;
}

FieldDoc::~FieldDoc(){
    if ( fields != nullptr ){
       for ( int i=0;fields[i]!=nullptr;i++ )
           _CLDELETE(fields[i]);
       _CLDELETE_ARRAY(fields);
    }
}



FieldDocSortedHitQueue::FieldDocSortedHitQueue (SortField** fields, int32_t size) {
	this->fields = fields;
	_countsize();
	//this->collators = hasCollators (fields);
	initialize (size,true);
}

bool FieldDocSortedHitQueue::lessThan (FieldDoc* docA, FieldDoc* docB) {
	int32_t n = fieldsLen;
	int32_t c = 0;
	qreal f1,f2,r1,r2;
	int32_t i1,i2;
	const TCHAR *s1, *s2;

	for (int32_t i=0; i<n && c==0; ++i) {
		int32_t type = fields[i]->getType();
		if (fields[i]->getReverse()) {
			switch (type) {
				case SortField::DOCSCORE:
					r1 = __REINTERPRET_CAST(Compare::Float*, docA->fields[i])->getValue();
					r2 = __REINTERPRET_CAST(Compare::Float*, docB->fields[i])->getValue();
					if (r1 < r2) c = -1;
					if (r1 > r2) c = 1;
					break;
				case SortField::DOC:
				case SortField::INT:
					i1 = __REINTERPRET_CAST(Compare::Int32*, docA->fields[i])->getValue();
					i2 = __REINTERPRET_CAST(Compare::Int32*, docB->fields[i])->getValue();
					if (i1 > i2) c = -1;
					if (i1 < i2) c = 1;
					break;
				case SortField::STRING:
					s1 = __REINTERPRET_CAST(Compare::TChar*, docA->fields[i])->getValue();
					s2 = __REINTERPRET_CAST(Compare::TChar*, docB->fields[i])->getValue();
					if (s2 == nullptr) c = -1;      // could be NULL if there are
					else if (s1 == nullptr) c = 1;  // no terms in the given field
					else c = _tcscmp(s2,s1); //else if (fields[i].getLocale() == NULL) {

					/*todo: collators not impl
					} else {
						c = collators[i].compare (s2, s1);
					}*/
					break;
				case SortField::FLOAT:
					f1 = __REINTERPRET_CAST(Compare::Float*, docA->fields[i])->getValue();
					f2 = __REINTERPRET_CAST(Compare::Float*, docB->fields[i])->getValue();
					if (f1 > f2) c = -1;
					if (f1 < f2) c = 1;
					break;
				case SortField::CUSTOM:
					c = docB->fields[i]->compareTo (docA->fields[i]);
					break;
				case SortField::AUTO:
					// we cannot handle this - even if we determine the type of object (qreal or
					// Integer), we don't necessarily know how to compare them (both SCORE and
					// qreal both contain floats, but are sorted opposite of each other). Before
					// we get here, each AUTO should have been replaced with its actual value.
					_CLTHROWA (CL_ERR_Runtime,"FieldDocSortedHitQueue cannot use an AUTO SortField");
				default:
					_CLTHROWA (CL_ERR_Runtime, "invalid SortField type"); //todo: rich error... : "+type);
			}
		} else {
			switch (type) {
				case SortField::DOCSCORE:
					r1 = __REINTERPRET_CAST(Compare::Float*, docA->fields[i])->getValue();
					r2 = __REINTERPRET_CAST(Compare::Float*, docB->fields[i])->getValue();
					if (r1 > r2) c = -1;
					if (r1 < r2) c = 1;
					break;
				case SortField::DOC:
				case SortField::INT:
					i1 = __REINTERPRET_CAST(Compare::Int32*, docA->fields[i])->getValue();
					i2 = __REINTERPRET_CAST(Compare::Int32*, docB->fields[i])->getValue();
					if (i1 < i2) c = -1;
					if (i1 > i2) c = 1;
					break;
				case SortField::STRING:
					s1 = __REINTERPRET_CAST(Compare::TChar*, docA->fields[i])->getValue();
					s2 = __REINTERPRET_CAST(Compare::TChar*, docB->fields[i])->getValue();
					// NULL values need to be sorted first, because of how FieldCache.getStringIndex()
					// works - in that routine, any documents without a value in the given field are
					// put first.
					if (s1 == nullptr) c = -1;      // could be NULL if there are
					else if (s2 == nullptr) c = 1;  // no terms in the given field
					else c = _tcscmp(s1,s2); //else if (fields[i].getLocale() == NULL) {
					
					/* todo: collators not implemented } else {
						c = collators[i].compare (s1, s2);
					}*/
					break;
				case SortField::FLOAT:
					f1 = __REINTERPRET_CAST(Compare::Float*, docA->fields[i])->getValue();
					f2 = __REINTERPRET_CAST(Compare::Float*, docB->fields[i])->getValue();
					if (f1 < f2) c = -1;
					if (f1 > f2) c = 1;
					break;
				case SortField::CUSTOM:
					c = docA->fields[i]->compareTo (docB->fields[i]);
					break;
				case SortField::AUTO:
					// we cannot handle this - even if we determine the type of object (qreal or
					// Integer), we don't necessarily know how to compare them (both SCORE and
					// qreal both contain floats, but are sorted opposite of each other). Before
					// we get here, each AUTO should have been replaced with its actual value.
					_CLTHROWA (CL_ERR_Runtime,"FieldDocSortedHitQueue cannot use an AUTO SortField");
				default:
					_CLTHROWA (CL_ERR_Runtime,"invalid SortField type"); //todo: rich error... : "+type);
			}
		}
	}
	return c > 0;
}

void FieldDocSortedHitQueue::setFields (SortField** fields) {
	SCOPED_LOCK_MUTEX(THIS_LOCK)
	if (this->fields == nullptr) {
		this->fields = fields;
		_countsize();
		//this->collators = hasCollators (fields);
	}else if ( fields == nullptr )
		this->fields = nullptr;
}

FieldDocSortedHitQueue::~FieldDocSortedHitQueue(){
    if ( fields != nullptr ){
       for ( int i=0;fields[i]!=nullptr;i++ )
           _CLDELETE(fields[i]);
       _CLDELETE_ARRAY(fields);
    }
}

CL_NS_END

