/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "FastCharStream.h"

#include "CLucene/util/Reader.h"

CL_NS_DEF(util)

const int32_t FastCharStream::maxRewindSize = LUCENE_MAX_WORD_LEN*2;

  FastCharStream::FastCharStream(Reader* reader):
    pos(0),
    rewindPos(0),
	resetPos(0),
	col(1),
	line(1),
	input(reader)
  {
    input->mark(maxRewindSize);
  }
	FastCharStream::~FastCharStream()= default;
  void FastCharStream::readChar(TCHAR &c) {
	try{
		int32_t r = input->read();
		if ( r == -1 )
			input = nullptr;
		c = r;
	}catch(CLuceneError& err){
		if ( err.number() == CL_ERR_IO )
			input = nullptr;
		throw err;
	}
  }
  int FastCharStream::GetNext()
  {
 //   printf("getnext\n");
    if (input == nullptr ) // end of file
    {
      _CLTHROWA(CL_ERR_IO,"warning : FileReader.GetNext : Read TCHAR over EOS.");
    }
    // this is rather inefficient
    // implementing the functions from the java version of
    // charstream will be much more efficient.
	++pos;
    if ( pos > resetPos + maxRewindSize && rewindPos == 0) {
        // move the marker one position (~expensive)
        resetPos = pos-(maxRewindSize/2);
		if ( resetPos != input->reset(resetPos) )
			_CLTHROWA(CL_ERR_IO,"Unexpected reset() result");
        input->mark(maxRewindSize);
        input->skip((maxRewindSize/2) - 1);
    }
    TCHAR ch;
    readChar(ch);

    if (input == nullptr) { // eof
        return -1;
    }
    if (rewindPos == 0) {
        col += 1;
        if(ch == '\n') {
            line++;
            col = 1;
        }
    } else {
        rewindPos--;
    }
    return ch;
  }

  void FastCharStream::UnGet(){
//      printf("UnGet \n");
    if (input == nullptr) 
		return;
    if ( pos == 0 ) {
      _CLTHROWA(CL_ERR_IO,"error : No character can be UnGet");
    }
    rewindPos++;

	input->reset(pos-1);
    pos--;
  }

  int FastCharStream::Peek() {
    int c = GetNext();
    UnGet();
    return c;
  }

  bool FastCharStream::Eos() const {
	return input==nullptr;
  }

  int32_t FastCharStream::Column() const {
	return col;
  }

  int32_t FastCharStream::Line() const {
	return line;
  }
CL_NS_END
