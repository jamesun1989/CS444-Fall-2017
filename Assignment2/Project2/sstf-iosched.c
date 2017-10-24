#include <linux/blkdev.h>
#include <linux/bio.h>
#include <linux/elevator.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

struct clook_data{
	struct list_head queue;
};

static void clook_merged_requests(struct request_queue *q, struct request *rq, struct request *next){
	list_del_init(&next->queuelist);
}

static int clook_dispatch(struct request_queue *q, int force){

	struct clook_data *cd = q->elevator->elevator_data;
	char operation;

	if(!list_empty(&cd->queue)){
		struct request *rq;
		rq = list_entry(cd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q,rq);

		if(rq_data_dir(rq) == REQ_WRITE)
			operation = 'W';
		else
			operation = 'R';

		printk(KERN_INFO "C-Look Dispatch %c %lu\n", operation, blk_rq_pos(rq));

		return 1;
	}
	return 0;
}

static void clook_add_request(struct request_queue *q, struct request *rq)
{
        struct clook_data *cd = q->elevator->elevator_data;
        struct list_head *cur = NULL;
        struct request *a;
        char operation;

        list_for_each(cur, &cd->queue){
        	a = list_entry(cur, struct request, queuelist);
        	if(rq_end_sector(rq) < rq_end_sector(a)){
        		break;
        	}
        }

        list_add_tail(&rq->queuelist, cur);

        if(rq_data_dir(rq) == REQ_WRITE)
                operation = 'W';
        else
                operation = 'R';

        printk(KERN_INFO "C-Look add %c %lu\n", operation, blk_rq_pos(current));
}

static struct request *
clook_former_request(struct request_queue *q, struct request *rq)
{
        struct clook_data *cd = q->elevator->elevator_data;

        if (rq->queuelist.prev == &cd->queue)
                return NULL;
        return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
clook_latter_request(struct request_queue *q, struct request *rq)
{
        struct clook_data *cd = q->elevator->elevator_data;

        if (rq->queuelist.next == &cd->queue)
                return NULL;
        return list_entry(rq->queuelist.next, struct request, queuelist);
}

static int clook_init_queue(struct request_queue *q, struct elevator_type *e)
{
        struct clook_data *cd;
        struct elevator_queue *eq;

        eq = elevator_alloc(q, e);
        if (!eq)
                return -ENOMEM;

        cd = kmalloc_node(sizeof(*cd), GFP_KERNEL, q->node);
        if (!cd) {
                kobject_put(&eq->kobj);
                return -ENOMEM;
        }
        eq->elevator_data = cd;

        INIT_LIST_HEAD(&cd->queue);

        spin_lock_irq(q->queue_lock);
        q->elevator = eq;
        spin_unlock_irq(q->queue_lock);
        return 0;
}

static void clook_exit_queue(struct elevator_queue *e)
{
        struct clook_data *cd = e->elevator_data;

        BUG_ON(!list_empty(&cd->queue));
        kfree(cd);
}

static struct elevator_type elevator_clook = {
        .ops = {
                .elevator_merge_req_fn          = clook_merged_requests,
                .elevator_dispatch_fn           = clook_dispatch,
                .elevator_add_req_fn            = clook_add_request,
                .elevator_former_req_fn         = clook_former_request,
                .elevator_latter_req_fn         = clook_latter_request,
                .elevator_init_fn               = clook_init_queue,
                .elevator_exit_fn               = clook_exit_queue,
        },
        .elevator_name = "clook",
        .elevator_owner = THIS_MODULE,
};

static int __init clook_init(void)
{
        return elv_register(&elevator_clook);
}

static void __exit clook_exit(void)
{
        elv_unregister(&elevator_clook);
}

module_init(clook_init);
module_exit(clook_exit);


MODULE_AUTHOR("Group 35");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("CLOOK IO scheduler");