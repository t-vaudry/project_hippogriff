__kernel void setState(__global bool *dev_s, __global bool *temp)
{
    int i = get_global_id(0);
    dev_s[i] = temp[i];
}