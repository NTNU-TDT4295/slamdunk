
  #ifndef SPI_Slave_H
  #define SPI_Slave_H
  #include "wrapperregdriver.h"
  #include <map>
  #include <string>
  #include <vector>

  using namespace std;
  class SPI_Slave {
  public:
    SPI_Slave(WrapperRegDriver * platform) {
      m_platform = platform;
      attach();
    }
    ~SPI_Slave() {
      detach();
    }

      void set_btn3(AccelReg value) {writeReg(1, value);} 
  void set_buffer_reset(AccelReg value) {writeReg(2, value);} 
  AccelReg get_io4_led() {return readReg(3);} 
  AccelReg get_led3() {return readReg(4);} 
  void set_read_addr(AccelReg value) {writeReg(5, value);} 
  AccelReg get_read_data() {return readReg(6);} 
  AccelReg get_read_size() {return readReg(7);} 
  AccelReg get_signature() {return readReg(0);} 
  void set_spi_mosi(AccelReg value) {writeReg(8, value);} 
  void set_spi_sck(AccelReg value) {writeReg(9, value);} 
  void set_spi_ss(AccelReg value) {writeReg(10, value);} 
  AccelReg get_valid_buffer() {return readReg(11);} 


    map<string, vector<unsigned int>> getStatusRegs() {
      map<string, vector<unsigned int>> ret = { {"io4_led", {3}} ,  {"led3", {4}} ,  {"read_data", {6}} ,  {"read_size", {7}} ,  {"signature", {0}} ,  {"valid_buffer", {11}} };
      return ret;
    }

    AccelReg readStatusReg(string regName) {
      map<string, vector<unsigned int>> statRegMap = getStatusRegs();
      if(statRegMap[regName].size() != 1) throw ">32 bit status regs are not yet supported from readStatusReg";
      return readReg(statRegMap[regName][0]);
    }

  protected:
    WrapperRegDriver * m_platform;
    AccelReg readReg(unsigned int i) {return m_platform->readReg(i);}
    void writeReg(unsigned int i, AccelReg v) {m_platform->writeReg(i,v);}
    void attach() {m_platform->attach("SPI_Slave");}
    void detach() {m_platform->detach();}
  };
  #endif
      