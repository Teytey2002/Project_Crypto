#include <iostream>
#include <cstdint>
#include <bitset>
using std::cout;
using std::endl;

typedef struct State{
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;
}State;

uint64_t getIthBit(uint64_t num, int i){
    return (num >> i) & 1ULL;
}

void shakeColumns(State* s){
    uint64_t a,b,c,d;
    a = s->b^s->c^s->d;
    b = s->a^s->c^s->d;
    c = s->a^s->b^s->d;
    d = s->a^s->b^s->c;
    s->a = a; 
    s->b = b; 
    s->c = c; 
    s->d = d;
}

void preShiftRows(State* s){
    uint64_t a,b,c,d;
    a = s->a;
    b = (s->b >> 1) | (s->b << 63);
    c = (s->c >> 3) | (s->c << 61);
    d = (s->d >> 9) | (s->d << 55);
    s->a = a; 
    s->b = b; 
    s->c = c; 
    s->d = d;
}

void stirColumns(State* s){
    s->a ^= (s->b&s->c);
    s->b ^= (s->c&s->d);
    s->c ^= (s->d&s->a);
    s->d ^= (s->a&s->b);
}

void postShiftRows(State* s){
    uint64_t a,b,c,d;
    a = s->a;
    b = (s->b >> 1) | (s->b << 63);
    c = (s->c >> 5) | (s->c << 59);
    d = (s->d >> 25) | (s->d << 39);
    s->a = a; 
    s->b = b; 
    s->c = c; 
    s->d = d; 
}

void addRoundConstant(State* s, uint8_t r){
    uint64_t mask = (1ULL << r+1)-1;
    s->a ^= mask;
}  

int main(){
    uint64_t a,b,c,d;
    //a = 0x0123456789ABCDEF, b = 0x2222222222222222, c = 0x1231231231231231, d = 0xABCDABCDABCDABCD;
    a = 0x0000000000000000, b = 0x0000000000000000, c = 0x0000000000000000, d = 0x0000000000000000;
    State s = {a,b,c,d};
    for (int i=0; i<13; i++){
        shakeColumns(&s);
        preShiftRows(&s);
        stirColumns(&s);
        postShiftRows(&s);
        addRoundConstant(&s, i);
        cout << i << " : " << std::hex << s.a << " " << s.b << " " << s.c << " " << s.d << endl;
    }
    
    return 0;
}