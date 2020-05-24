
#ifndef CIRCLEBUFFER_H
#define CIRCLEBUFFER_H

#define CIRCLEBUFFER_SIZE 255
//----------------------------------------------------------------
//---------------------- Circular buffer -------------------------
//----------------------------------------------------------------
class CircleBuffer {
  private:
  uint8_t buffer[CIRCLEBUFFER_SIZE];
  uint16_t head;

  public:

  CircleBuffer() {
    head = 0;
  };

  void put(uint8_t data) {
    head = (head+1) % CIRCLEBUFFER_SIZE;
    buffer[head] = data;
  };

  uint8_t get(int16_t index) {
    int16_t real_index = (head - index);
    if(real_index > 0) real_index = real_index % CIRCLEBUFFER_SIZE;
    while(real_index < 0) real_index+=CIRCLEBUFFER_SIZE;
    return buffer[real_index];
  };
};
//----------------------------------------------------------------
#endif
