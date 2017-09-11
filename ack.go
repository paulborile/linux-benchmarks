package main

func main() {
	ack(3,12);
}


func ack ( m int, n int) int {
	if ( m == 0 ) {
		return ( n + 1 )
	} else {
		if ( n == 0 ) {
			return ack ( m - 1 , 1 )
		} else {
			return ack ( m - 1 , ack ( m , n - 1 ))
		}
	}
}


/*
int a;
main()
{
	int x;
	a = ack (3,12);
}


int	ack ( int m , int n )
{
	if ( m == 0 ) return ( n + 1 );
	else 
		if ( n == 0 ) return ack ( m - 1 , 1 );
		else  return ack ( m - 1 , ack ( m , n - 1 ));
}
*/
