
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

      void set_btn(AccelReg value) {writeReg(1, value);} 
  AccelReg get_led3() {return readReg(2);} 
  AccelReg get_lidar_burst_counter() {return readReg(3);} 
  void set_read_addr(AccelReg value) {writeReg(4, value);} 
  AccelReg get_read_data() {return readReg(5);} 
  AccelReg get_signature() {return readReg(0);} 
  void set_spi_mosi(AccelReg value) {writeReg(6, value);} 
  void set_spi_ready(AccelReg value) {writeReg(7, value);} 
  void set_spi_sck(AccelReg value) {writeReg(8, value);} 
  void set_spi_ss(AccelReg value) {writeReg(9, value);} 
  AccelReg get_spi_valid() {return readReg(10);} 


    map<string, vector<unsigned int>> getStatusRegs() {
      map<string, vector<unsigned int>> ret = { {"led3", {2}} ,  {"lidar_burst_counter", {3}} ,  {"read_data", {5}} ,  {"signature", {0}} ,  {"spi_valid", {10}} };
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
      