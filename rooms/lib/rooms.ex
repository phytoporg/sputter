defmodule Rooms do
  use Application

  @impl true
  def start(_type, _args) do
    Rooms.Supervisor.start_link(name: Rooms.Supervisor)
  end
end
