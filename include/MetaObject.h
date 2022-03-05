#pragma once
	
namespace meta{
namespace vision{
	
	class MetaObject{
	public:
	    MetaObject(){}
	    virtual ~MetaObject(){}
	public:    
	    /**
	     * MetaObject
	     * BaseFrame 继承 MetaObject  BaseFrame的type为1
         * MetaData 继承MetaObject   Whale的type为2
	     * Event 继承MetaObject       Event的type为3
	     * 
	    **/
	    int obj_type;
	};
}//meta
}//vision