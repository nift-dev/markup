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
 expect(".. _home: https://example.test\n\nVisit home_.\n","<p>Visit <a href=\"https://example.test\">home</a>.</p>\n");
 expect("Text [1]_.\n\n.. [1] Note.\n","<p>Text <a class=\"footnote-reference\" href=\"#footnote-1\">[1]</a>.</p>\n<aside class=\"footnote\" id=\"footnote-1\"><span>[1]</span> Note.</aside>\n");
 expect("See [CITE]_.\n\n.. [CITE] Source.\n","<p>See <a class=\"footnote-reference\" href=\"#footnote-CITE\">[CITE]</a>.</p>\n<aside class=\"citation\" id=\"footnote-CITE\"><span>[CITE]</span> Source.</aside>\n");
 expect("Hello |name|.\n\n.. |name| replace:: world\n","<p>Hello world.</p>\n");
 expect(".. hidden comment\n   continued\n\nVisible.\n","<p>Visible.</p>\n");
 expect("=====  =====\nName   Count\n=====  =====\nBolt   4\n=====  =====\n","<table class=\"simple\">\n<tr><td>Name</td><td>Count</td></tr>\n<tr><td>Bolt</td><td>4</td></tr>\n</table>\n");
 expect("+------+-------+\n| Name | Count |\n+------+-------+\n| Bolt | 4     |\n+------+-------+\n","<table class=\"grid\">\n<tr><td>Name</td><td>Count</td></tr>\n<tr><td>Bolt</td><td>4</td></tr>\n</table>\n");
 expect("Use :code:`x` :math:`a+b` :emphasis:`e` :strong:`s` :sub:`2` :sup:`3` :title:`Book`","<p>Use <code class=\"code\">x</code> <span class=\"math\">a+b</span> <em>e</em> <strong>s</strong> <sub>2</sub> <sup>3</sup> <cite>Book</cite></p>\n");
 {markup::Options options;std::vector<std::string>d;options.rst_diagnostic=[&](const std::string&v){d.push_back(v);};std::string a,b;if(!markup::convert(markup::Format::ReStructuredText,"Use :sphinx:`x`",a,b,options)||d.size()!=1)return 4;++checks;}
 expect(".. note::\n\n   Remember this.\n","<aside class=\"admonition note\"><p class=\"admonition-title\">note</p><p>Remember this.</p></aside>\n");
 expect(".. topic:: Details\n\n   Topic body.\n","<section class=\"topic\"><h2>Details</h2><p>Topic body.</p></section>\n");
 expect(".. code:: cpp\n\n   int x;\n","<pre class=\"code cpp\">int x;</pre>\n");
 {markup::Options options;std::vector<std::string>deps;options.rst_resource_resolver=[](const std::string&,const std::string&t,std::string&c,std::string&id,std::string&){if(t!="part.rst")return false;c="Included.\n";id="virtual:part";return true;};options.rst_dependency=[&](const std::string&v){deps.push_back(v);};std::string a,b;if(!markup::convert(markup::Format::ReStructuredText,"Before.\n\n.. include:: part.rst\n\nAfter.\n",a,b,options)||a!="<p>Before.</p>\n<p>Included.</p>\n<p>After.</p>\n"||deps.size()!=1)return 5;++checks;}
 {markup::Options options;options.rst_resource_resolver=[](const std::string&,const std::string&,std::string&c,std::string&id,std::string&){c=".. include:: loop.rst\n";id="virtual:loop";return true;};std::string a,b;if(markup::convert(markup::Format::ReStructuredText,".. include:: loop.rst\n",a,b,options)||b.find("cycle")==std::string::npos)return 6;++checks;}
 expect(".. image:: image.png\n","<figure><img src=\"image.png\" alt=\"\"></figure>\n");
 std::string o,e;if(!markup::is_supported(markup::Format::ReStructuredText)||!markup::convert(markup::Format::ReStructuredText,"repeat",o,e))return 2;auto first=o;if(!markup::convert(markup::Format::ReStructuredText,"repeat",o,e)||o!=first)return 3;checks+=2;
 std::cout<<checks<<" reStructuredText checks passed\n";
}
