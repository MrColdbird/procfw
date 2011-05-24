/*
	PSP VSH extender for devhook 0.50+
*/
#include "common.h"

int limit(int val,int min,int max)
{
	if(val<min) val = max;
	if(val>max) val = min;
	return val;
}

/*
int get_max_len(char **str_list,int nums)
{
	int max_len = 0;
	int i;
	for(i=0;i<nums;i++)
	{
		int len;

#ifdef CONFIG_639
		if(psp_fw_version == FW_639)
			len = scePaf_strlen(str_list[i]);
#endif

#ifdef CONFIG_635
		if(psp_fw_version == FW_635)
			len = scePaf_strlen(str_list[i]);
#endif

#ifdef CONFIG_620
		if(psp_fw_version == FW_620)
			len = scePaf_strlen_620(str_list[i]);
#endif
	
		if(max_len < len) max_len = len;
	}
	return max_len;
}
*/
