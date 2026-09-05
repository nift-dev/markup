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
 expect("Title\n=====\n\nSection\n-------\n\nBody.\n","<section id=\"title\">\n<h1>Title</h1>\n<section id=\"section\">\n<h2>Section</h2>\n<p>Body.</p>\n</section>\n</section>\n");
 expect("Before.\n\n----\n\nAfter.\n","<p>Before.</p>\n<hr>\n<p>After.</p>\n");
 expect("   quoted line\n   second\n","<blockquote>\n<p>quoted line\nsecond</p>\n</blockquote>\n");
 expect("*emphasis* **strong** ``literal <&>``\n","<p><em>emphasis</em> <strong>strong</strong> <code>literal &lt;&amp;&gt;</code></p>\n");
 expect("`site <https://example.test>`_ and https://example.test/path\n","<p><a href=\"https://example.test\">site</a> and <a href=\"https://example.test/path\">https://example.test/path</a></p>\n");
 expect("Use :code:`value` and |name|.\n","<p>Use <code class=\"code\">value</code> and |name|.</p>\n");
 expect("word*not emphasis*word and \\*escaped*\n","<p>word*not emphasis*word and *escaped*</p>\n");
 expect("Example::\n\n   literal <tag>\n   second\n","<p>Example:</p>\n<pre class=\"literal-block\">literal &lt;tag&gt;\nsecond</pre>\n");
 expect("| first\n| second\n","<div class=\"line-block\">\n<div class=\"line\">first</div>\n<div class=\"line\">second</div>\n</div>\n");
 expect(">>> 1 + 1\n2\n","<pre class=\"doctest-block\">&gt;&gt;&gt; 1 + 1\n2</pre>\n");
 expect("* one\n* two\n","<ul>\n<li>one</li>\n<li>two</li>\n</ul>\n");
 expect("1. one\n2. two\n","<ol>\n<li>one</li>\n<li>two</li>\n</ol>\n");
 expect("Term\n  Definition.\n","<dl>\n<dt>Term</dt>\n<dd>Definition.</dd>\n</dl>\n");
 expect(":Author: Ada\n:Version: 1\n","<dl class=\"field-list\">\n<dt>Author</dt>\n<dd>Ada</dd>\n<dt>Version</dt>\n<dd>1</dd>\n</dl>\n");
 std::string o,e;if(!markup::is_supported(markup::Format::ReStructuredText)||!markup::convert(markup::Format::ReStructuredText,"repeat",o,e))return 2;auto first=o;if(!markup::convert(markup::Format::ReStructuredText,"repeat",o,e)||o!=first)return 3;checks+=2;
 std::cout<<checks<<" reStructuredText checks passed\n";
}
