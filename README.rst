This packages contains a simple C++ tokenizing and parsing framework. As a
sample of how to use the framework it has a parser for ISC style configuration
files as used by bind and ISC dhcpd.

The code is split in two parts: a tokenizer that extracts tokens of various
types (integer, strings, keywords, whitespace and other characters) from its
input and a token handler which will be called by the tokenizer for each token.

The current code is very prelimary; the API, filenames and pretty much
everything else can change in the next few versions. The current release is
already useable though.

Simple example
--------------

Below is a simple example of how to use the ISC parser. It reads two files:
defaults which includes all default settings and config which has the current
configuration. Those are then merged to create a single ConfigData instance
with the complete configuration.

::

   boost::shared_ptr<ConfigData> ReadConfig(const char *fn) {
       MemoryFile input(fn);
       Tokenizer toker(input);
       ISCParser parser;
   
       toker(dynamic_cast<TokenHandler&>(parser));
       return parser.cfg;
   }
   
   int main(int argc, char** argv) {
       boost::shared_ptr<ConfigData> defaults;
       boost::shared_ptr<ConfigData> settings;
   
       defaults=ReadConfig("defaults");
       settings=ReadConfig("config");
       settings->Merge(*defaults, false, true);
   
       int port = (*settings)["RADIUS"]["server"]["port"];


Changes
-------

0.2
  Add code to merge ConfigData instances, which can be used to implement defaults settings and type-checking for values.

0.1
  First release

