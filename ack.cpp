class Ack {
public:
	Ack();
	~Ack();
	int Run( int m , int n );
};

Ack::Ack()
{
}

Ack::~Ack()
{
}

int Ack::Run( int m , int n )
{
	if ( m == 0 ) return ( n + 1 );
	else if ( n == 0 ) return Run ( m - 1 , 1 );
	else  return Run ( m - 1 , Run ( m , n - 1 ));
}

int main()
{
	int b;
	Ack a;

	b = a.Run ( 3 ,12 );
}
