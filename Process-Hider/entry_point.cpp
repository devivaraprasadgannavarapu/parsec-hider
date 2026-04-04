#include "process_hider/external/external.hpp"
#include "process_hider/internal/internal.hpp"

auto __stdcall entry_point( sdk::loader* loader ) -> __int32
{
	sdk::handler::create( );

	if ( !sdk::is_valid_loader( loader ) )
	{
		__int32 target_pid = 0;

		for ( ; !sdk::win::get_async_key_state( VK_END ); sdk::win::sleep( 100 ) )
		{
			__int32 current_pid = ph::external::get_process_pid_by_name( xorstr( "Taskmgr.exe" ) );

			if ( current_pid != 0 && current_pid != target_pid )
			{
				if ( target_pid != 0 )
					ph::external::unload( );

				sdk::win::sleep( 1000 );

				current_pid = ph::external::get_process_pid_by_name( xorstr( "Taskmgr.exe" ) );

				if ( current_pid != 0 && current_pid != target_pid )
				{
					ph::external::inject( );
					target_pid = current_pid;
				}
			}
			else if ( current_pid == 0 && target_pid != 0 )
			{
				ph::external::unload( );
				target_pid = 0;
			}
		}

		if ( target_pid != 0 )
			ph::external::unload( );
	}

	else
	{
		for (
			ph::internal::enable_hook( loader );
			!sdk::win::get_async_key_state( VK_END );
			sdk::win::sleep( 1 ) );

		ph::internal::disable_hook( );
	}

	sdk::handler::destroy( );

	return 0;
}