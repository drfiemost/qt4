/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "RangeQuery.h"

#include "SearchHeader.h"
#include "Scorer.h"
#include "BooleanQuery.h"
#include "TermQuery.h"

#include "CLucene/index/Term.h"
#include "CLucene/index/Terms.h"
#include "CLucene/index/IndexReader.h"
#include "CLucene/util/StringBuffer.h"


CL_NS_USE(index)
CL_NS_USE(util)
CL_NS_DEF(search)

	RangeQuery::RangeQuery(Term* lowerTerm, Term* upperTerm, const bool Inclusive){
	//Func - Constructor
	//Pre  - (LowerTerm != NULL OR UpperTerm != NULL) AND
	//       if LowerTerm and UpperTerm are valid pointer then the fieldnames must be the same
	//Post - The instance has been created

		if (lowerTerm == nullptr && upperTerm == nullptr)
        {
            _CLTHROWA(CL_ERR_IllegalArgument,"At least one term must be non-null");
        }
        if (lowerTerm != nullptr && upperTerm != nullptr && lowerTerm->field() != upperTerm->field())
        {
            _CLTHROWA(CL_ERR_IllegalArgument,"Both terms must be for the same field");
        }

		// if we have a lowerTerm, start there. otherwise, start at beginning
        if (lowerTerm != nullptr) {
            this->lowerTerm = _CL_POINTER(lowerTerm);
        }
        else {
            this->lowerTerm = _CLNEW Term(upperTerm, LUCENE_BLANK_STRING);
        }
        this->upperTerm = (upperTerm != nullptr ? _CL_POINTER(upperTerm) : nullptr);
        this->inclusive = Inclusive;
    }
	RangeQuery::RangeQuery(const RangeQuery& clone):
		Query(clone){
		this->inclusive = clone.inclusive;
		this->upperTerm = (clone.upperTerm != nullptr ? _CL_POINTER(clone.upperTerm) : nullptr );
		this->lowerTerm = (clone.lowerTerm != nullptr ? _CL_POINTER(clone.lowerTerm) : nullptr );
	}
	Query* RangeQuery::clone() const{
	  return _CLNEW RangeQuery(*this);
	}

    RangeQuery::~RangeQuery() {
    //Func - Destructor
    //Pre  - true
    //Post - The instance has been destroyed

        _CLDECDELETE(lowerTerm);
        _CLDECDELETE(upperTerm);
    }

	/** Returns a hash code value for this object.*/
    size_t RangeQuery::hashCode() const {
			return Similarity::floatToByte(getBoost()) ^
	            (lowerTerm != nullptr ? lowerTerm->hashCode() : 0) ^
	            (upperTerm != nullptr ? upperTerm->hashCode() : 0) ^
	            (this->inclusive ? 1 : 0);
    }

    const TCHAR* RangeQuery::getQueryName() const{
      return getClassName();
    }
	const TCHAR* RangeQuery::getClassName(){
		return _T("RangeQuery");
	}

	Query* RangeQuery::combine(Query** queries) {
		return Query::mergeBooleanQueries(queries);
	}
	
	bool RangeQuery::equals(Query * other) const{
	  if (!(other->instanceOf(RangeQuery::getClassName())))
            return false;

        RangeQuery* rq = (RangeQuery*)other;
		bool ret = (this->getBoost() == rq->getBoost())
			&& (this->isInclusive() == rq->isInclusive())
			&& (this->getLowerTerm()->equals(rq->getLowerTerm()))
			&& (this->getUpperTerm()->equals(rq->getUpperTerm()));

		return ret;
	}


	/**
     * FIXME: Describe <code>rewrite</code> method here.
     *
     * @param reader an <code>IndexReader</code> value
     * @return a <code>Query</code> value
     * @exception IOException if an error occurs
     */
    Query* RangeQuery::rewrite(IndexReader* reader){

        BooleanQuery* query = _CLNEW BooleanQuery;
        TermEnum* enumerator = reader->terms(lowerTerm);
		Term* lastTerm = nullptr;
        try {
            bool checkLower = false;
            if (!inclusive) // make adjustments to set to exclusive
                checkLower = true;

            const TCHAR* testField = getField();
            do {
                lastTerm = enumerator->term();
                if (lastTerm != nullptr && lastTerm->field() == testField ) {
                    if (!checkLower || _tcscmp(lastTerm->text(),lowerTerm->text()) > 0) {
                        checkLower = false;
                        if (upperTerm != nullptr) {
                            int compare = _tcscmp(upperTerm->text(),lastTerm->text());
                            /* if beyond the upper term, or is exclusive and
                             * this is equal to the upper term, break out */
                            if ((compare < 0) || (!inclusive && compare == 0))
                                break;
                        }
                        TermQuery* tq = _CLNEW TermQuery(lastTerm); // found a match
                        tq->setBoost(getBoost()); // set the boost
                        query->add(tq, true, false, false); // add to query
                    }
                }else {
                    break;
                }
				_CLDECDELETE(lastTerm);
            }
            while (enumerator->next());
		}catch(...){
			_CLDECDELETE(lastTerm); //always need to delete this
			_CLDELETE(query); //in case of error, delete the query
            enumerator->close();
			_CLDELETE(enumerator);
			throw; //rethrow
		}
		_CLDECDELETE(lastTerm); //always need to delete this
		enumerator->close();
		_CLDELETE(enumerator);

        return query;
    }

    /** Prints a user-readable version of this query. */
    TCHAR* RangeQuery::toString(const TCHAR* field) const
    {
        StringBuffer buffer;
        if ( field==nullptr || _tcscmp(getField(),field)!=0 )
        {
            buffer.append( getField() );
            buffer.append( _T(":"));
        }
        buffer.append(inclusive ? _T("[") : _T("{"));
        buffer.append(lowerTerm != nullptr ? lowerTerm->text() : _T("NULL"));
        buffer.append(_T(" TO "));
        buffer.append(upperTerm != nullptr ? upperTerm->text() : _T("NULL"));
        buffer.append(inclusive ? _T("]") : _T("}"));
        if (getBoost() != 1.0f)
        {
            buffer.append( _T("^"));
            buffer.appendFloat( getBoost(),1 );
        }
        return buffer.toString();
    }


    const TCHAR* RangeQuery::getField() const	
	{
		return (lowerTerm != nullptr ? lowerTerm->field() : upperTerm->field());
	}

	/** Returns the lower term of this range query */
    Term* RangeQuery::getLowerTerm(bool pointer) const { 
		if ( pointer )
			return _CL_POINTER(lowerTerm); 
		else
			return lowerTerm;
	}

    /** Returns the upper term of this range query */
    Term* RangeQuery::getUpperTerm(bool pointer) const { 
		if ( pointer )
			return _CL_POINTER(upperTerm); 
		else
			return upperTerm;
	}

    /** Returns <code>true</code> if the range query is inclusive */
    bool RangeQuery::isInclusive() const { return inclusive; }


CL_NS_END
