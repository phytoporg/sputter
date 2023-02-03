defmodule Rooms.Supervisor do
  use Supervisor

  def start_link(opts) do
    Supervisor.start_link(__MODULE__, :ok, opts)
  end

  @impl true
  def init(:ok) do
    children = [
      {DynamicSupervisor, name: Rooms.BucketSupervisor, strategy: :one_for_one},
      {Rooms.Registry, name: Rooms.Registry},
    ]

    Supervisor.init(children, strategy: :one_for_all)
  end

end
