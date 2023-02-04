defmodule Rooms.Supervisor do
  use Supervisor

  def start_link(opts) do
    Supervisor.start_link(__MODULE__, :ok, opts)
  end

  @impl true
  def init(:ok) do
    children = [
      {Rooms.Registry, name: Rooms.Registry},
      # {Rooms.Bucket, name: {:via, Registry, {Rooms.Registry, :bucket}}},
    ]

    Supervisor.init(children, strategy: :one_for_all)
  end

end
