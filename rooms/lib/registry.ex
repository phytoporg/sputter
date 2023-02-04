defmodule Rooms.Registry do
  use Agent, restart: :temporary
  @doc """
  Starts a new registry.

  `:name` is required.
  """
  def start_link(opts) do
    name = Keyword.fetch!(opts, :name)
    Registry.start_link(keys: :unique, name: name)
  end

  
  def lookup(server, name) do
    case Registry.lookup(server, name) do
      [] -> :error
      [{pid, _}] -> {:ok, pid}
    end
  end

  def create(server, name) do
    {:ok, _} = Registry.register(server, name, {Rooms.Bucket, name: name})
  end

end

