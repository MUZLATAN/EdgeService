#pragma once
	
namespace meta{
namespace vision{
	
	class AlgoObject{
	public:
	    AlgoObject(){}
	    virtual ~AlgoObject(){}
	public:    
	    /**
	     * AlgoObject
	     * BaseFrame 继承 AlgoObject  BaseFrame的type为1
         * AlgoData 继承AlgoObject   Whale的type为2
	     * Event 继承AlgoObject       Event的type为3
	     * 
	    **/
	    int obj_type;
	};
}//meta
}//vision