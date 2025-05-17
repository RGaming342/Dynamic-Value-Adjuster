# Dynamic-Value-Adjuster
Dynamic-Value-Adjuster or DVA is used to change/set values at run time, its simple, easy to use, and header only library
<pre>
Declare DVA with:  
  DVA name;

there are 6 supported value types:  
  int, uint, float, double, bool, std::string

to link a value to DVA use one of the 6 link functions:  
  DVA::addInt(name,&val);  
  DVA::addUint(name,&val);  
  DVA::addFloat(name,&val);  
  DVA::addDouble(name,&val);  
  DVA::addBool(name,&val);  
  DVA::addString(name,&val);  

thats it, those linked values can now be changed at runtime.
</pre>
## Commands
<pre>
  set name value // sets the value of name to value
  help           // lists all commands(like this list)
  list           // lists all linked variables and there values
</pre>

## Example:
<pre>
  DVA dva;
  int val=0;
  
  dva.addInt("val",&amp;val);

  while(true){
    std::cout<< "val: " << val <&lt; std::endl;
  }
</pre>
