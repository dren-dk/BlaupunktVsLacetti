#ifndef REMOTE_H
#define REMOTE_H

typedef struct {

  unsigned int gap;    // Time between each code start.
  
  unsigned char fixedBits; // fixed Bits in each code, max 16.
  unsigned int fixed;      // The actual fixed bits.
  unsigned char bits;      // Unique bits in each code, max 16.

  // Mark:  electrically low  or modulation present
  // Space: electrically high or modulation silent

  // Signal for the header which is sent before every code
  unsigned int headerMark;
  unsigned int headerSpace;

  // Signal for a binary 1
  unsigned int oneMark;    
  unsigned int oneSpace;

  // Signal for a binary 0
  unsigned int zeroMark;
  unsigned int zeroSpace;

  // Signal sent 
  unsigned int repeatMark;
  unsigned int repeatSpace;

  // tail mark sent after each code
  unsigned int tailMark;
} Remote;

typedef struct {
  const Remote *remote;
  unsigned int code;
  unsigned char state; 
  // 0: Header mark
  // 1: Header space
  // 2: Code
  // 3: Tail mark
  // 4: Gap space
  // 5: Post gap

  unsigned char codeState;
  unsigned char repeating;
  unsigned int length;
} RemoteState;

/*
  Start a new remote code, pre-allocate the RemoteState structure.
*/
void remoteStart(RemoteState *rs, const Remote *remote, unsigned int code);  

/*
  Returns true if the current remote code is done and the remote is ready for
  a new code.

  If you keep transmitting after remoteDone is true, the signal will repeat.
*/
char remoteDone(RemoteState *rs);

/*
  Returns the length of the mark or space to transmit now, in us and advances the state-machine. 

  If < 0 then it's a space, if it's > 0 then it's a mark.
*/
int remoteSignal(RemoteState *rs);

#endif
