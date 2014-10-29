#include "tiny_mm.h"

/*
 * @brief   
 *   Arrange memory that could been use
 *   XXX: this buffer could only been used in specific situation
 * @inputs  
 * @outputs 
 * @retval  
 */
void recg_buf_arrange(struct st_recg_buf *recg_buf, uint8_t *buf, uint32_t n_bytes)
{
    recg_buf->buf     = buf;
    recg_buf->max_len = n_bytes;    
    recg_buf->cur_len = 0;
}

/*
 * @brief   
 *   Arrange memory that could been use
 *   XXX: this buffer could only been used in specific situation
 * @inputs  
 * @outputs 
 * @retval  
 */
void *get_recg_buf(struct st_recg_buf *recg_buf, int n_bytes)
{
    void *ret = (void *)0;
    
    ret = recg_buf->buf + recg_buf->cur_len;
    recg_buf->cur_len += n_bytes;

    if (recg_buf->cur_len > recg_buf->max_len) {
        recg_buf->cur_len -= n_bytes;
        return (void *)0;
    } else {
        return ret;
    }
}

/*
 * @brief   
 *   Arrange memory that could been use
 *   XXX: this buffer could only been used in specific situation
 * @inputs  
 * @outputs 
 * @retval  
 */
void put_recg_buf(struct st_recg_buf *recg_buf, int n_bytes)
{
    recg_buf->cur_len -= n_bytes;
}

