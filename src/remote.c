#include "remote.h"

void remoteStart(RemoteState *rs, const Remote *remote, unsigned int code) {
  rs->remote = remote;
  rs->code = code;
  rs->state = 0;
  rs->codeState = 0;
  rs->repeating = 0;
  rs->length = 0;
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
    rs->state++;
    rs->length += rs->remote->repeatSpace;
    return -rs->remote->repeatSpace;
  }
}

int remoteCodeSignal(RemoteState *rs) {

  char space = rs->codeState & 1;
  char bit   = rs->codeState >> 1;
  char one;

  if (bit < rs->remote->fixedBits) {    
    one = rs->remote->fixed & (1<<(rs->remote->fixedBits-bit-1));
    rs->codeState++;

  } else {
    bit -= rs->remote->fixedBits;
    one = rs->code & (1<<(rs->remote->bits - bit - 1));
    rs->codeState++;

    if (bit >= rs->remote->bits) {
      rs->state++;
    }
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
    rs->state++;
    rs->length += rs->remote->headerMark;
    return rs->remote->headerMark;

  } else if (rs->state == 1) {
    rs->state++;
    rs->length += rs->remote->headerSpace;
    return -rs->remote->headerSpace;

  } else if (rs->state == 2) {

    if (rs->repeating) {
      return remoteRepeatSignal(rs);
    } else {
      return remoteCodeSignal(rs);
    }

  } else if (rs->state == 3) {
    rs->state++;
    rs->length += rs->remote->tailMark;
    return rs->remote->tailMark;
    
  } else if (rs->state == 4) {
    rs->state++;    
    return -(rs->remote->gap - rs->length);

  } else {
    // Fail!!!
    rs->state = 5; // A small hack to avoid hanging up.
    return 0;
  }
}
