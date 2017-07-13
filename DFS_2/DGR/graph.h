#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

using namespace std;

inline off_t fsize(const char *filename) {
	struct stat st;
	if (stat(filename, &st) == 0)
		return st.st_size;
	return -1;
}

template<
typename file_vert_t, typename file_index_t,
typename new_vert_t, typename new_index_t>
class graph
{
	public:
		new_index_t *beg_pos = NULL;
		new_vert_t *csr=NULL;
		new_index_t vert_count = 0;
		new_index_t edge_count = 0;

	public:
		graph(){};
		~graph(){};
		graph(const char *beg_file, 
				const char *csr_file){
			//double tm=wtime();
		FILE *file=NULL;
		file_index_t ret;
		
		vert_count=fsize(beg_file)/sizeof(file_index_t) - 1;
		edge_count=fsize(csr_file)/sizeof(file_vert_t);
		
		file=fopen(beg_file, "rb");
		if(file!=NULL)
		{
			file_index_t *tmp_beg_pos = new file_index_t[vert_count+1];
			ret=fread(tmp_beg_pos, sizeof(file_index_t), 
					vert_count+1, file);
			assert(ret==vert_count+1);
			fclose(file);
			std::cout<<"Possible edge count: "<<tmp_beg_pos[vert_count]<<"\n";
		
			//converting to new type when different 
			if(sizeof(file_index_t)!=sizeof(new_index_t))
			{
				beg_pos = new new_index_t[vert_count+1];
				for(new_index_t i=0;i<vert_count+1;++i)
					beg_pos[i]=(new_index_t)tmp_beg_pos[i];
				delete[] tmp_beg_pos;
			}else{beg_pos=(new_index_t*)tmp_beg_pos;}

		}else std::cout<<"beg file cannot open\n";

		file=fopen(csr_file, "rb");
		if(file!=NULL)
		{
			file_vert_t *tmp_csr = NULL;
			if(posix_memalign((void **)&tmp_csr,32,sizeof(file_vert_t)*edge_count))
				perror("posix_memalign");
			
			ret=fread(tmp_csr, sizeof(file_vert_t), edge_count, file);
			assert(ret==edge_count);
			fclose(file);
				
			if(sizeof(file_vert_t)!=sizeof(new_vert_t))
			{
				if(posix_memalign((void **)&csr,32,sizeof(new_vert_t)*edge_count))
					perror("posix_memalign");
				for(new_index_t i=0;i<edge_count;++i)
					csr[i]=(new_vert_t)tmp_csr[i];
				delete[] tmp_csr;
			}else csr=(new_vert_t*)tmp_csr;

		}else std::cout<<"CSR file cannot open\n";

		cout<<"Graph load (success): "<<vert_count<<" verts, "
			<<edge_count<<" edges"<<endl;
	}
};