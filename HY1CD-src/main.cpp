#include "hy1_czi_l2_pro.hpp"

int main(int argc, char**argv)
{

	// Run the Debug
	if(argc == 1){
		argc=2;
		argv[1] = "/home/zhongyong/program/jobOrder/hy1_czi_l2_pro.param.xml";
	}
    // Return code declaration
    PIK_Int_t iRet;
    // Check the right number of arguments
    if(CUTIL_PIKUsage::usage(argc) == PIK_FALSE)
    {
        // Select the corresponding return code
        iRet = E_INIT_ERR_ARGS;
    }
    else
    {
        // Init the return code
        iRet = 0;
        HY1_CZI_L2A_PRO oProcessing;
       // Init the execution handler
        CGEN_ExecutionHandler *poHanlder =
                    CGEN_ExecutionHandler::initExecHandler(argv[1],oProcessing);
        if(poHanlder!=NULL)
        {
            // Now run the processing, include the PreProcessing, Processing  and PostProcessing
            iRet=poHanlder->start();
            delete poHanlder;
            poHanlder=NULL;

            // Run the signal 15 processing
            oProcessing.cancel();
        }
        else
        {
            iRet=CGEN_ExecutionHandler::_iStaticRet;
        }
    }
    return iRet;
}
