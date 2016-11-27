__kernel void hello_kernel(__global bool *a,
						__global const bool *b)
{
    int gid = get_global_id(0);

    if(gid%4 == 0)
    	a[gid] = a[gid] && b[gid];
    else if(gid%4 == 1)
    	a[gid] = a[gid] || b[gid];
    else if(gid%4 == 2)
    	a[gid] = (a[gid] && !b[gid]) || (!a[gid] && b[gid]); 
    else
    	a[gid] = (a[gid] && b[gid]) || (!a[gid] && !b[gid]);  	
}
