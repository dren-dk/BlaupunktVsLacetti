#include "remote.h"

void remoteStart(RemoteState *rs, const Remote *remote, unsigned int code) {
  rs->remote = remote;
  rs->state = 0;
  rs->codeState = 0;
  rs->repeating = 0;
  rs->length = 0;

  for (int i=0;i<4;i++) {
	  rs->code[i] = 0;
  }

  int bits = 0;
  for (int i=rs->remote->fixedBits-1;i>=0;i--) {
	  if (rs->remote->fixed & (1<<i)) {
		  rs->code[bits>>3] |= 1<<(bits & 7);
	  }
	  bits++;
  }

  for (int i=rs->remote->bits-1;i>=0;i--) {
	  if (code & (1<<i)) {
		  rs->code[bits>>3] |= 1<<(bits & 7);
	  }
	  bits++;
  }
}

char remoteDone(RemoteState *rs) {
  return rs->state == 5;
}

int remoteRepeatSignal(RemoteState *rs) {
  if (rs->codeState == 0) {
    rs->codeState++;
    rs->length += rs->remote->repeatMark;
    return rs->remote->repeatMark;
    
  } else  {
    rs->state = 3;
    rs->length += rs->remote->repeatSpace;
    return -rs->remote->repeatSpace;
  }
}

int remoteCodeSignal(RemoteState *rs) {

  char space = rs->codeState & 1;
  char bit   = rs->codeState >> 1;
  char one   = rs->code[bit>>3] & 1<<(bit & 7);;

  rs->codeState++;
  if (rs->codeState >= (rs->remote->fixedBits + rs->remote->bits) << 1) {
      rs->state++;
  }

  if (space) {
    if (one) {
      rs->length += rs->remote->oneSpace;
      return -rs->remote->oneSpace;
    } else {
      rs->length += rs->remote->zeroSpace;
      return -rs->remote->zeroSpace;
    }
  } else {
    if (one) {
      rs->length += rs->remote->oneMark;
      return rs->remote->oneMark;
    } else {
      rs->length += rs->remote->zeroMark;
      return rs->remote->zeroMark;
    }
  }
}

int remoteSignal(RemoteState *rs) {
  if (rs->state == 5) {
    rs->state = 0;
    rs->repeating = 1;
    rs->codeState = 0;
    rs->length = 0;
  } // No else here, because we immediately fall into state 0 to repeat

  if (rs->state == 0) {

	  if (rs->repeating) {
		  return remoteRepeatSignal(rs);
	  } else {
		  rs->state++;
		  rs->length += rs->remote->headerMark;
		  return rs->remote->headerMark;
	  }

  } else if (rs->state == 1) {
    rs->state++;
    rs->length += rs->remote->headerSpace;
    return -rs->remote->headerSpace;

  } else if (rs->state == 2) {
      return remoteCodeSignal(rs);

  } else if (rs->state == 3) {
    rs->state++;
    rs->length += rs->remote->tailMark;
    rs->codeState = 0;
    return rs->remote->tailMark;
    
  } else if (rs->state == 4) {

	if (!rs->codeState) {
		rs->length = rs->remote->gap-rs->length;
		rs->codeState = 1;
	}

	if (rs->length > 32000) {
		rs->length -= 32000;
		return -32000;

	} else {

	    rs->state++;
	    return -rs->length;
	}

  } else {
    // Fail!!!
    rs->state = 5; // A small hack to avoid hanging up.
    return 0;
  }
}
