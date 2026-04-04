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
			if ( target_pid != 0 )
			{
				if ( ph::external::process_handle != nullptr && sdk::win::wait_for_single_object( ph::external::process_handle, 0 ) == 0 )
				{
					// Process has ended, safely cleanup the handle
					ph::external::unload( );
					target_pid = 0;
					ph::external::process_handle = nullptr;
				}
			}

			if ( target_pid == 0 )
			{
				__int32 current_pid = ph::external::get_process_pid_by_name( xorstr( "Taskmgr.exe" ) );

				if ( current_pid != 0 )
				{
					sdk::win::sleep( 1000 );

					current_pid = ph::external::get_process_pid_by_name( xorstr( "Taskmgr.exe" ) );

					if ( current_pid != 0 )
					{
						ph::external::inject( );
						target_pid = current_pid;
					}
				}
			}
		}

		if ( target_pid != 0 && ph::external::process_handle != nullptr )
		{
			// We only call unload manually if we are terminating the program while taskmgr is alive,
			// though it crashes Task Manager so it's a bit dangerous. Since VK_END unloads, it's original behavior.
			// Here we just close the handle rather than unloading memory to prevent a crash.
			sdk::win::close_handle( ph::external::process_handle );
		}
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