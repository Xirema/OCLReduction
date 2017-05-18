R"D(
kernel void reduce(global Scalar * a, global Scalar * output, local Scalar * scratch, global long * index_output, local long * index_scratch, long size) {
	size_t gid = get_global_id(0);
	size_t lid = get_local_id(0);
	size_t wid = get_group_id(0);
	size_t gsize = get_global_size(0);
	size_t lsize = get_local_size(0);
	size_t wsize = get_num_groups(0);

	if(gid < size) {
		scratch[lid] = a[gid];
		index_scratch[lid] = gid;
	} else {
		scratch[lid] = IDENTITY;
		index_scratch[lid] = -1;
	}
		
	barrier(CLK_LOCAL_MEM_FENCE);
	for(size_t offset = lsize / 2; offset > 0; offset >>= 1) {
		if(lid < offset) {
			size_t indexa = index_scratch[lid];
			size_t indexb = index_scratch[lid + offset];
			Scalar a = scratch[lid];
			Scalar b = scratch[lid + offset];

			Scalar reduced_value;
			size_t reduced_index;

			REDUCE_IMPL(a, b, indexa, indexb, reduced_value, reduced_index);

			scratch[lid] = reduced_value;
			index_scratch[lid] = reduced_index;
		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	if(lid == 0) {
		output[wid] = scratch[0];
		index_output[wid] = index_scratch[0];
	}
}
)D"