# CN-Final-CA
The final project for computer network course at university of Tehran


# questions
## Part 2.3

>1. What are the main differences between UDP and TCP protocols? When shoud we use each one?

### Differences:
The biggest difference is TCP requiering a connection while UDP is connectionless. The second biggest difference is reliability; TCP guarantess delivery while UDP uses best effort. In other words TCP will retransmit lost packets while UDP doesn't. Another important difference is method of transfer meaning TCP transfers a byte stream while UDP transfers packets as such the order of delivery must be handled by the user in UDP. UDP has no flow control mechanism but TCP does. UDP can handle broadcast but TCP cannot. TCP uses more errorchecking than UDP. UDP has a fixed length header while TCP uses variable length one.



| category            | TCP                 | UDP             |
| -------------       | -------------       | -------------   |
| connection          | established         | connectionless  |
| reliability         | guarantee delivery  | best effort     |
| order               | guarantee in order  | no guarantee    |
| flow clontrol       | yes                 | no              |
| broadcast/multicast | no                  | yes             |
| faster              | no                  | yes             |
| more error checking | yes                 | no              |
| header size         | up to 60 bytes      | fixed 8 bytes   |


### Application:
In short TCP is more useful when we need to guarantee the data is transmitted completely and correctly this would include transmiting:
* Code or text 
* Pictures
* Video
* Web pages
* File transfer
* email
* etc.
And UDP is more useful when we don't need to guarantee complete and correct data transfer as much as we need low latency (live data) and where a few dropped packets would not too heavily impact the results this would include :
* Live video/audio broadcast
* Gaming
* etc.

>2.What are the advantages and disadvantages of Go-Back-N and Selective Repeat? State the reasons for your choice.

In Go-Back-N we retransmit everything after the last acknowledged frame this would mean that if multiple frames were dropped or corrupted we would not need to identify each one but it also means potentially retransmitting a lot of extra data depending on window size and the number of actuall dropped/corrupted frames. In particualr if we drop the first or second frame in a window and everythin else is transmitted correctly we would retransmit the whole window. This approach is a lot less complex than the alternative and so is more used in the real world.

In Selective Repeat we only retransmit the frames that were not correctly transmitted in the first place. This requires that we acknowledge the successfully transmitted frames individually which need the transmition of extra data but could save transmitting extra data in the future. On the other hand this means that if we happen to drop a lot of frames we need to wait for the receiver to timeout each one and tell us to retransmit them which could result in less than optimal utilization.

In short it's better to use Selective Repeat when we have a good connection and we know we won't be dropping a lot of frames and use Go-Back-N when we have a bad connection and are expecting to lose a lot of frames. Due to the less complex nature of Go-Back-N it is used more in practice.


## Part 3.1.3
>1. Do you know a better way to update the table after a change in topology?

We make each node send a periodic update message even if nothing has changed this lets the neighboring nodes know that the link is active and we allow the nodes to send a message to *trigger* an update in the case of a node failing. We can also use the methods known as **split horizon** and **split horizon with poison reverse**
