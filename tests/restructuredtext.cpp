#include <markup/Markup.h>
#include <cstdlib>
#include <iostream>
#include <string>
namespace { int checks=0; void expect(const std::string&s,const std::string&e){std::string o,x;if(!markup::convert(markup::Format::ReStructuredText,s,o,x)||o!=e){std::cerr<<"RST mismatch\n"<<o<<"error: "<<x<<"\n";std::exit(1);}++checks;} }
int main(){
 expect("","");
 expect("A paragraph.\n","<p>A paragraph.</p>\n");
 expect("first\r\nsecond\rthird","<p>first\nsecond\nthird</p>\n");
 expect("caf\xc3\xa9","<p>caf\xc3\xa9</p>\n");
 expect(std::string("a\0b",3),"<p>a\xef\xbf\xbd" "b</p>\n");
 expect("<tag> & text","<p>&lt;tag&gt; &amp; text</p>\n");
 std::string o,e;if(!markup::is_supported(markup::Format::ReStructuredText)||!markup::convert(markup::Format::ReStructuredText,"repeat",o,e))return 2;auto first=o;if(!markup::convert(markup::Format::ReStructuredText,"repeat",o,e)||o!=first)return 3;checks+=2;
 std::cout<<checks<<" reStructuredText checks passed\n";
}
